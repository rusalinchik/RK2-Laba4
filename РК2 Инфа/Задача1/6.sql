WITH student_info AS (
    SELECT student_id, full_name, group_number
    FROM students 
    WHERE student_id = 3
)
SELECT 
    s.student_id,
    s.full_name,
    s.group_number,
    CASE 
        WHEN s.student_id < si.student_id THEN 'до'
        WHEN s.student_id > si.student_id THEN 'после'
        ELSE 'сам студент'
    END as position
FROM students s
CROSS JOIN student_info si
WHERE s.group_number = si.group_number 
  AND (
      (s.student_id BETWEEN si.student_id - 2 AND si.student_id - 1) OR
      (s.student_id BETWEEN si.student_id + 1 AND si.student_id + 3) OR
      s.student_id = si.student_id
  )
ORDER BY s.student_id;

CREATE OR REPLACE VIEW student_avg_grades AS
SELECT 
    s.student_id,
    s.full_name,
    s.group_number,
    ROUND(AVG(g.grade)::numeric, 2) as average_grade,
    COUNT(g.grade_id) as total_grades
FROM students s
LEFT JOIN grades g ON s.student_id = g.student_id
GROUP BY s.student_id, s.full_name, s.group_number
ORDER BY average_grade DESC;

SELECT * FROM student_avg_grades WHERE full_name = 'Альхо Альберт Петрович';

SELECT 
    s.subject_name,
    ROUND(AVG(g.grade)::numeric, 2) as average_grade,
    COUNT(g.grade_id) as total_grades,
    MIN(g.grade) as min_grade,
    MAX(g.grade) as max_grade
FROM grades g
JOIN subjects s ON g.subject_id = s.subject_id
WHERE s.subject_name = 'Информатика'
GROUP BY s.subject_id, s.subject_name;

ALTER TABLE notes ADD COLUMN IF NOT EXISTS note_search_vector tsvector;
UPDATE notes SET note_search_vector = to_tsvector('russian', note_text);
CREATE INDEX IF NOT EXISTS idx_notes_note_search_vector ON notes USING gin(note_search_vector);

SELECT 
    n.note_id,
    s.full_name,
    n.note_text,
    n.created_at
FROM notes n
JOIN students s ON n.student_id = s.student_id
WHERE note_search_vector @@ to_tsquery('russian', 'Информатика')
ORDER BY n.created_at DESC;

SELECT 
    n.note_id,
    s.full_name,
    n.note_text,
    n.created_at
FROM notes n
JOIN students s ON n.student_id = s.student_id
WHERE n.note_text ILIKE '%Информатика%'
ORDER BY n.created_at DESC;

BEGIN TRANSACTION;

SELECT * FROM attendance 
WHERE student_id = 5 AND date_attended = '2025-01-15';

UPDATE attendance 
SET status = 'present'
WHERE student_id = 5 AND date_attended = '2025-01-15';

SELECT * FROM attendance 
WHERE student_id = 5 AND date_attended = '2025-01-15';

COMMIT;